```markdown
# Battery Hardware Interface (ROS 2)

Ce package fournit une interface matérielle (`ros2_control` **SensorInterface**) pour surveiller l'état de la batterie d'un robot via un capteur **INA226** et un Arduino.

Ce package fait partie d'une suite d'outils (Toolkit) pour la robotique.

## 🏗️ Architecture

Le système lit la tension, le courant et la puissance via I2C sur l'Arduino. L'interface C++ parse la trame série et publie directement un message standard `sensor_msgs/msg/BatteryState` sur le topic `/battery_state`.

```text
+-------------------------------------------------------------+
|                       ORDINATEUR (ROS 2)                    |
|                                                             |
|  [/battery_state] <── Publie sensor_msgs/msg/BatteryState   |
|         ▲                                                   |
|         │                                                   |
|  [battery_hardware (C++ SensorInterface)]                   |
|         │                                                   |
|         │ 1. Parsing de la trame série                      |
|         │ 2. Publication directe du message ROS 2           |
|         ▼                                                   |
|  [libserial] ──────────────┐                                |
+----------------------------|--------------------------------+
                             | USB Série (115200 bauds)
                             | Trame: "V:12.45 I:1.250 P:15.56"
                             ▼
+-------------------------------------------------------------+
|                       ARDUINO (C++)                         |
|                                                             |
|  [Adafruit_INA226.h] <── [Bus I2C] <── [Capteur INA226]     |
+-------------------------------------------------------------+
```

## 📂 Structure du Package

```text
battery_hardware/
├── CMakeLists.txt
├── package.xml
├── battery_hardware.xml        # Manifeste du plugin
├── arduino/
│   └── battery_firmware.ino    # Code Arduino (Lecture I2C)
├── config/
│   └── controllers.yaml        # Configuration du update_rate
├── include/battery_hardware/
│   └── battery_interface.hpp
├── launch/
│   └── battery_test.launch.py
├── scripts/
│   └── battery_monitor.py      # Script de surveillance (alertes)
├── src/
│   └── battery_interface.cpp   # Code C++ (publication BatteryState)
└── urdf/
    ├── battery_robot.urdf.xacro
    └── battery_control.xacro
```

## ⚙️ Fonctionnalités Clés

- **Publication Directe** : Le C++ instancie un nœud ROS 2 en interne pour publier directement le message standard `BatteryState`. Pas besoin de nœud Python intermédiaire.
- **Moniteur de Sécurité** : Le script `battery_monitor.py` surveille la tension et affiche des alertes (WARN/ERROR) si la batterie passe sous un seuil critique.
- **SensorInterface** : Utilise la classe `hardware_interface::SensorInterface`, le standard pour les capteurs passifs dans `ros2_control`.

## 🚀 Utilisation

**1. Téléverser le code Arduino**
Ouvrez `arduino/battery_firmware.ino` dans l'IDE Arduino et téléversez-le (nécessite la bibliothèque `Adafruit INA226`). Vérifiez la valeur de votre résistance de shunt dans le code (souvent `0.1` ohm pour les modules standards).

**2. Compiler et lancer**
```bash
colcon build --packages-select battery_hardware
source install/setup.bash
ros2 launch battery_hardware battery_test.launch.py
```

**3. Vérifier les données (dans un autre terminal)**
```bash
ros2 topic echo /battery_state
```

**4. Lancer le moniteur de sécurité**
```bash
ros2 run battery_hardware battery_monitor.py
```

## 👨‍💻 Auteur
Lacina Sidibe (PFE 2027)
