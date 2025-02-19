import 'dart:async';
import 'dart:io';
import 'dart:convert';

import 'package:backend_app/database.dart';
import 'package:backend_app/module_log_model.dart';

class ClientService {
  final MyDatabase db;

  late WebSocket _socket;

  late final Completer<void> _completer;
  ModuleLogModel log = ModuleLogModel(lightStatus: 0);
  final _controller = StreamController<ModuleLogModel>.broadcast();

  ClientService(String url, this.db) {
    _init();
  }

  void _init() async {
    _completer = Completer<void>();
    print('Connecting to WebSocket Server...');
    _socket = await WebSocket.connect('ws://172.16.0.3:81');
    _completer.complete();

    _socket.listen((event) {
      if (event is String) {
        try {
          final data = jsonDecode(event);
          try {
            log = ModuleLogModel.fromJson(data);
            _controller.sink.add(log);
          } catch (e) {
            print(e);
          }
        } catch (_) {}
      }
    });
  }

  Future<void> get ready => _completer.future;
  Stream<ModuleLogModel> get stream => _controller.stream;

  send(String event) {
    print('Sent to server: $event');
    _socket.add(event);
  }
}
