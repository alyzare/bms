class ScenarioModel {
  final int id;
  final SensorType sensorType;
  final double minValue;
  final double maxValue;
  final int order;

  ScenarioModel({
    required this.id,
    required this.sensorType,
    required this.minValue,
    required this.maxValue,
    required this.order,
  });

  Map<String,dynamic> toJson() {
    return {
      'id': id,
      'sensorType': sensorType.name,
      'minValue': minValue,
      'maxValue': maxValue,
      'order': order,
    };
  }
}

enum SensorType {
  temperature(-20, 60, "temperature"),
  humidity(0, 100, "humidity"),
  gas(0, 100, "gas"),
  ;

  final double minValue;
  final double maxValue;
  final String name;

  const SensorType(this.minValue, this.maxValue, this.name);
}
