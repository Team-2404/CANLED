import wpilib

class CANLED:
    def __init__(self, id: int):
        self.id = id
        self.device = wpilib.CAN(id)
        self.api_id = 473
    
    def solid_color(self, red : int, green : int, blue : int):
        color = [0,red, green, blue]
        self.device.writePacket(color[0].to_bytes()+color[1].to_bytes()+color[2].to_bytes()+color[3].to_bytes(), self.api_id)
    
    def rainbow_cycle(self):
        self.device.writePacket((1).to_bytes(), self.api_id)

    def rainbow_solid(self):
        self.device.writePacket((2).to_bytes(), self.api_id)

