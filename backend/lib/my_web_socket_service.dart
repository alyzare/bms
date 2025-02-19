import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:backend_app/client_service.dart';
import 'package:backend_app/database.dart';
import 'package:backend_app/scenario_model.dart';
// import 'package:shelf_web_socket/shelf_web_socket.dart' as sh;

class MyWebSocketService {
  final int port;
  final ClientService clientService;
  final MyDatabase db;

  MyWebSocketService(this.port, this.clientService, this.db);

  Future<void> start() async {
    final server = await HttpServer.bind(InternetAddress.anyIPv4, port);
    print("WebSocket server started on port: ${server.port}");

    server.listen(
      (request) async {
        if (WebSocketTransformer.isUpgradeRequest(request)) {
          final webSocket = await WebSocketTransformer.upgrade(request);
          print("new websocket on port: ${request.connectionInfo?.remotePort}");
          webSocket.listen(
            (event) async {
              if (event is! String || !event.startsWith('\r\n\r\n')) return;

              final data = event.substring(4);
              final orderRegexp = RegExp(r'[01]{4}');
              final addScenarioRegexp = RegExp(r'^addScenario\{.*\}$');
              final deleteScenariosRegexp = RegExp(r'^deleteScenario\[\d+\]$');
              if (orderRegexp.hasMatch(data)) {
                clientService
                    .send("\r\n\r\n${orderRegexp.firstMatch(data)![0]!}");
              } else if (addScenarioRegexp.hasMatch(data)) {
                try {
                  final scenarioJson = jsonDecode(data.substring(11));
                  final valueMin = scenarioJson['value_min'] is int
                      ? scenarioJson['value_min'].toDouble()
                      : scenarioJson['value_min'];
                  final valueMax = scenarioJson['value_max'] is int
                      ? scenarioJson['value_max'].toDouble()
                      : scenarioJson['value_max'];
                  final sensorType = scenarioJson['sensor_type'];
                  final action = scenarioJson['action'];

                  final scenario = ScenarioModel(
                    id: -1,
                    sensorType: SensorType.values.firstWhere(
                      (element) => element.name == sensorType,
                    ),
                    minValue: valueMin,
                    maxValue: valueMax,
                    order: action,
                  );
                  db.insertScenario(scenario);
                  await Future.delayed(Duration(milliseconds: 200));
                  final scenarios = db.getScenarios();
                  final scenarioJsons =
                      scenarios.map((e) => e.toJson()).toList();
                  webSocket.add("scenarios${jsonEncode(scenarioJsons)}");
                } catch (e) {
                  print('Invalid JSON: $e');
                }
              } else if (deleteScenariosRegexp.hasMatch(data)) {
                final id = int.parse(data.substring(
                  data.indexOf('[') + 1,
                  data.indexOf(']'),
                ));
                db.deleteScenario(id);
              } else if (data.contains('getScenarios')) {
                final scenarios = db.getScenarios();
                final scenarioJsons = scenarios.map((e) => e.toJson()).toList();
                webSocket.add("scenarios${jsonEncode(scenarioJsons)}");
              }
            },
          );

          final sub = clientService.stream.listen(
            (event) {
              webSocket.add("module_log${jsonEncode(event.toJson())}");
            },
          );
          webSocket.done.then(
            (_) {
              sub.cancel();
              print('WebSocket closed on port: ${request.headers.port}');
            },
          );
        }
      },
    );
  }
}
