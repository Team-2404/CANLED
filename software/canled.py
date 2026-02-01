"""
CANLED WPILIB Python API

Author: Conor Kelly Gerakos
Team: FRC 2404
License: GPL v3.0
Year: 2025
"""

import wpilib

class CANLED:
    """A class for controlling the CANLED board."""
    def __init__(self, can_id: int):
        self.can_id = can_id
        self.device = wpilib.CAN(can_id)
        self.api_id = 0b01010000010000000000000000000 + can_id
        self.wavenum = 4
        self.solidnum = 3

    def solid_color(self, red : int, green : int, blue : int):
        """Sets all LEDs to a solid color"""
        color = [1, red, green, blue]
        self.device.writePacket(color[0].to_bytes()+color[1].to_bytes()+color[2].to_bytes()+color[3].to_bytes(), self.api_id)

    def rainbow_cycle(self, wait = 10):
        """Rainbow 'wave' of sorts. Waits for (wait) ms during loop."""
        # colors need to be fixed
        self.device.writePacket((self.wavenum).to_bytes()+wait.to_bytes(), self.api_id)

    def rainbow_solid(self, wait = 10):
        """All LEDs cycle through the same colors. Waits for (wait) ms during loop."""
        # currently acts strangely
        self.device.writePacket((self.solidnum).to_bytes()+wait, self.api_id)
