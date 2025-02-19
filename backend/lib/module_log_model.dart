class ModuleLogModel {
  final double? temp;
  final double? humidity;
  final double? gas;
  final int lightStatus;

  ModuleLogModel({
    this.temp,
    this.humidity,
    this.gas,
    required this.lightStatus,
  });

  factory ModuleLogModel.fromJson(Map<String, dynamic> json) {
    return ModuleLogModel(
      temp: json['temperature'] is int ? json['temperature'].toDouble() : json['temperature'],
      humidity: json['humidity'] is int ? json['humidity'].toDouble() : json['humidity'],
      gas: json['gas'] is int ? json['gas'].toDouble() : json['gas'],
      lightStatus: json['lightStatus'],
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'temperature': temp,
      'humidity': humidity,
      'gas': gas,
      'lightStatus': lightStatus,
    };
  }
}
