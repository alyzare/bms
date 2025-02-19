import 'dart:async';

import 'package:backend_app/broadcast_service.dart';
import 'package:backend_app/client_service.dart';
import 'package:backend_app/database.dart';
import 'package:backend_app/my_web_socket_service.dart';

void main() async {
  MyDatabase db = MyDatabase();

  BroadcastService broadcastService = BroadcastService(1380);
  final moduleIpAddress = await broadcastService.getFirstAddress();
  late final ClientService clientService;
  if (moduleIpAddress == null) {
    return;
  }

  clientService = ClientService("ws://$moduleIpAddress:81", db);
  await clientService.ready;
  print('Connected to module at $moduleIpAddress');

  await MyWebSocketService(
    8081,
    clientService,
    db,
  ).start();

  Timer.periodic(Duration(seconds: 1), (timer) {
    checkFullfilledScenarios(db, clientService);
  });
}

void checkFullfilledScenarios(MyDatabase db, ClientService clientService) {
  final fulfilledScenarios = db.getFulfilledScenarios(clientService.log);

  final latestScenario = fulfilledScenarios?.firstOrNull;
  if (latestScenario == null) return;

  final action = latestScenario['action'] as int;
  final first4Bits = (action >> (action.bitLength - 4)) & 0xF;
  final order = "\r\n\r\n${first4Bits.toRadixString(2).padLeft(4, '0')}";
  clientService.send(order);
}
