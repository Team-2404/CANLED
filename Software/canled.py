import wpilib

class CANLED:
    def __init__(self, id: int):
        self.id = id
        self.device = wpilib.CAN(id)
        self.api_id = 473
        self.wavenum = 4
        self.solidnum = 3
    
    def solid_color(self, red : int, green : int, blue : int):
        """Sets all LEDs to a solid color"""
        color = [1,red, green, blue]
        self.device.writePacket(color[0].to_bytes()+color[1].to_bytes()+color[2].to_bytes()+color[3].to_bytes(), self.api_id)
    
    def rainbow_cycle(self):
        """Rainbow 'wave' of sorts."""
        # colors need to be fixed
        self.device.writePacket((wavenum).to_bytes(), self.api_id)

    def rainbow_solid(self):
        """All LEDs cycle through the same colors."""
        # currently acts strangely
        self.device.writePacket((solidnum).to_bytes(), self.api_id)
        
    def rainbow_cycle(self, wait:int):
         """Rainbow 'wave' of sorts.  Waits for (wait) ms before changing."""
        self.device.writePacket((wavenum).to_bytes()+wait.to_bytes(), self.api_id)

    def rainbow_solid(self, wait:int):
         """All LEDs cycle through the same colors. Waits for (wait) ms before changing."""
        self.device.writePacket((solidnum).to_bytes()+wait.to_bytes(), self.api_id)

