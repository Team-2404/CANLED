package frc.robot.subsystems;
/* CANLED WPILIB Java API

Author: Conor Kelly Gerakos
Team: FRC 2404
License: GPL v3.0
Year: 2025
Usage: Place into subsystems folder.
*/
import edu.wpi.first.wpilibj.CAN;

/**
 * This class is used to interact with the physical CANLED board.
 * It provides methods to set the connected LED strip to colors or patterns.
 */
public class CANLED{
    private int frcId;
    private final int rainbow_wave = 4;
    private final int rainbow_solid = 3;
    private final int apiId = 473;
    private CAN canled;

    /**
     * Constructor for CANLED class.
     * @param id The FRC CAN id of the CANLED board set in the .ino code uploaded to it.
     */
    public CANLED(int id){
        frcId = id;
        canled = new CAN(frcId);       
    }

    /**
     * Constructor for CANLED class. Uses default FRC CAN id value of 55.
     */
    public CANLED(){
        frcId = 55;
        canled = new CAN(55);
    }

    /** 
    * Rainbow where colors cycle through the strip.
    * @param wait the time to wait in ms between loop iterations
    */
    public void rainbowCycle(int wait){
        byte[] msg = {rainbow_wave,(byte)wait};
        canled.writePacket(msg, apiId);
    }

    /** 
    * Rainbow where entire strip is one color which cycles through rainbow.
    * @param wait the time in ms to wait between loop iterations
    */
    public void rainbowSolid(int wait){
        byte[] msg = {rainbow_solid, (byte)wait};
        canled.writePacket(msg, apiId);
    }
    
    /** 
    * Rainbow where colors cycle through the strip.
    * Waits 10 ms between loop iterations.
    */
    public void rainbowCycle(){
        byte[] msg = {rainbow_wave,(byte)10};
        canled.writePacket(msg, apiId);
    }
    
    /** 
    * Rainbow where entire strip is one color which cycles through rainbow.
    * Waits 10 ms between loop iterations.
    */
    public void rainbowSolid(){
        byte[] msg = {rainbow_solid, (byte)10};
        canled.writePacket(msg, apiId);
    }

    /**
     * Sets entire strip to solid color.
     * @param r Red byte for strip to display
     * @param g Green byte for strip to display
     * @param b Blue byte for strip to display
     */
    public void setColor(byte r, byte g, byte b){
        byte[] msg = {1, r, g, b};
        canled.writePacket(msg, apiId);
    }
}
