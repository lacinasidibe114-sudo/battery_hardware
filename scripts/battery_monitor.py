#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import BatteryState

class BatteryMonitor(Node):
    def __init__(self):
        super().__init__('battery_monitor')
        self.subscription = self.create_subscription(
            BatteryState,
            '/battery_state',
            self.battery_callback,
            10)
        self.get_logger().info('Moniteur de batterie démarré...')
        
        # Paramètres de ta batterie (ex: LiPo 3S)
        self.max_voltage = 12.6
        self.min_voltage = 9.9 # 3.3V par cellule (seuil critique)

    def battery_callback(self, msg):
        # Calcul du pourcentage (approximatif pour une LiPo)
        percentage = max(0.0, min(1.0, (msg.voltage - self.min_voltage) / (self.max_voltage - self.min_voltage)))
        
        if percentage < 0.15:
            self.get_logger().error(
                f"⚠️ BATTERIE CRITIQUE ⚠️ | Tension: {msg.voltage:.2f}V | Courant: {msg.current:.2f}A"
            )
        elif percentage < 0.30:
            self.get_logger().warn(
                f"🪫 Batterie faible | Tension: {msg.voltage:.2f}V | Niveau: {percentage*100:.1f}%"
            )
        else:
            self.get_logger().info(
                f"🔋 Batterie OK | Tension: {msg.voltage:.2f}V | Courant: {msg.current:.2f}A | Niveau: {percentage*100:.1f}%"
            )

def main(args=None):
    rclpy.init(args=args)
    node = BatteryMonitor()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()