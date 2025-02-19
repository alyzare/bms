import 'package:backend_app/module_log_model.dart';
import 'package:backend_app/scenario_model.dart';
import 'package:sqlite3/sqlite3.dart';

class MyDatabase {
  late Database _db;

  MyDatabase() {
    _db = sqlite3.open('backend.db');

    _db.execute('''
      CREATE TABLE IF NOT EXISTS scenarios (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        value_min REAL,
        value_max REAL,
        sensor_type TEXT,
        action INT);
    ''');
  }

  void insertScenario(ScenarioModel scenario) {
    _db.execute('''
      INSERT INTO scenarios (value_min, value_max, sensor_type, action) VALUES (?, ?, ?, ?);
    ''', [
      scenario.minValue,
      scenario.maxValue,
      scenario.sensorType.name,
      scenario.order,
    ]);
  }

  ResultSet? getFulfilledScenarios(ModuleLogModel log) {
    final conditions = <String>[];
    final parameters = <Object?>[];

    if (log.temp == null && log.humidity == null && log.gas == null) {
      return null;
    }

    if (log.temp != null) {
      conditions.add(
        'sensor_type = \'temp\' AND value_min <= ? AND value_max >= ?',
      );
      parameters.addAll([log.temp, log.temp]);
    }
    if (log.humidity != null) {
      conditions.add(
        'sensor_type = \'humidity\' AND value_min <= ? AND value_max >= ?',
      );
      parameters.addAll([log.humidity, log.humidity]);
    }
    if (log.gas != null) {
      conditions.add(
        'sensor_type = \'gas\' AND value_min <= ? AND value_max >= ?',
      );
      parameters.addAll([log.gas, log.gas]);
    }

    return _db.select('''
      SELECT * FROM scenarios
      WHERE ${conditions.join(' OR ')};
    ''', parameters);
  }

  List<ScenarioModel> getScenarios() {
    final scenarios = _db.select('''
      SELECT * FROM scenarios;
    ''');

    return scenarios.map((e) {
      return ScenarioModel(
        id: e['id'],
        sensorType: SensorType.values.firstWhere(
          (element) => element.name == e['sensor_type'],
        ),
        minValue: e['value_min'],
        maxValue: e['value_max'],
        order: e['action'],
      );
    }).toList();
  }

  void deleteScenario(int id) {
    _db.execute('''
      DELETE FROM scenarios WHERE id = ?;
    ''', [id]);
  }

  void close() {
    _db.dispose();
  }
}
