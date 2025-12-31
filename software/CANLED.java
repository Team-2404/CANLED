package frc.robot.subsystems;
/* CANLED WPILIB Python API

Author: Conor Kelly Gerakos
Team: FRC 2404
License: GPL v3.0
Year: 2025
Usage: Place into subsystems folder.
*/

import edu.wpi.first.wpilibj.CAN;

public class CANLED{
    private int frcId;
    private final int rainbow_wave = 4;
    private final int rainbow_solid = 3;
    private final int apiId = 473;
    private CAN canled;
    public CANLED(int id){
        frcId = id;
        canled = new CAN(frcId);       
    }

    public CANLED(){
        frcId = 55;
        canled = new CAN(55);
    }

    public void rainbowCycle(int wait){
        byte[] msg = {rainbow_wave,(byte)wait};
        canled.writePacket(msg, apiId);
    }

    public void rainbowSolid(int wait){
        byte[] msg = {rainbow_solid, (byte)wait};
        canled.writePacket(msg, apiId);
    }

    public void setColor(byte r, byte g, byte b){
        byte[] msg = {1, r, g, b};
        canled.writePacket(msg, apiId);
    }
}
