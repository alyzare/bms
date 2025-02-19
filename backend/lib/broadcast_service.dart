import 'dart:async';
import 'dart:io';

class BroadcastService {
  final int _port;
  RawDatagramSocket? _socket;

  BroadcastService(this._port);

  FutureOr<InternetAddress?> getFirstAddress() async {
    print("udp socket binding...");
    _socket = await RawDatagramSocket.bind(InternetAddress.anyIPv4, _port);
    await for (RawSocketEvent event in _socket!) {
      if (event == RawSocketEvent.read) {
        final datagram = _socket!.receive();
        print('datagram recieved: ${datagram?.data}');

        if (datagram != null &&
            String.fromCharCodes(datagram.data) == "BMS_HARDWARE") {
          print('returning address: ${datagram.address}');
          return datagram.address;
        }
      }
    }
    return null;
  }
}
