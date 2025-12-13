package CANLED;
//  By Conor Kelly Gerakos, Team 2404
//  For usage with the CANLED board.


import edu.wpi.first.wpilibj.CAN;

public class CANLED{
    private int frcId;
    private final int rainbow_wave = 1;
    private final int rainbow_solid = 2;
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

    public void rainbowCycle(){
        byte[] msg = {rainbow_wave};
        canled.writePacket(msg, apiId);
    }

    public void rainbowSolid(){
        byte[] msg = {rainbow_solid};
        canled.writePacket(msg, apiId);
    }

    public void setColor(byte r, byte g, byte b){
        byte[] msg = {0, r, g, b};
        canled.writePacket(msg, apiId);
    }
}
