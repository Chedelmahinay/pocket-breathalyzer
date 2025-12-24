// Execute BLE connection on clien side; server cant do it
"use client";

import { useState } from "react";
import { Button } from "@/components/ui/button";

const SERVICE_UUID = "000000ff-0000-1000-8000-00805f9b34fb";
const CHARACTERISTIC_UUID = "0000ff01-0000-1000-8000-00805f9b34fb";

//connecton status
export function BleConnectButton() {
  const [status, setStatus] = useState<"idle" | "connecting" | "connected">(
    "idle"
  );

  //triggered by user click
  async function connect() {
    try {
      setStatus("connecting");

      //scan for device with the name X and advertising same UUID
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ name: "ESP_GATTS_DEMO" }],
        optionalServices: [SERVICE_UUID],
      });

      const server = await device.gatt!.connect();  //establish BLE connection; ! = gatt is not null(TypeScript)
      const service = await server.getPrimaryService(SERVICE_UUID);
      const characteristic =
        await service.getCharacteristic(CHARACTERISTIC_UUID); //requiered for notifications or writes

      //subscribe to notifications
      await characteristic.startNotifications();

      //triggered when esp32 sends notification
      characteristic.addEventListener(
        "characteristicvaluechanged",
        (event: Event) => {
          const target =
            event.target as BluetoothRemoteGATTCharacteristic; //event target type so TypeScripts is not bitching
          const value = target.value!; //value is DataView holding raw bytes
          //bytes as 32-bit unsingend int & little endian
          const dv = new DataView(value.buffer);
          const data = dv.getUint32(0, true);
          console.log("Received:", data);
        }
      );

      setStatus("connected"); //update UI
      console.log("BLE connected");
    } catch (err) {
      console.error("BLE error:", err); //log errors
      setStatus("idle");
    }
  }

  return (
    //onClick={connect} = user gesture (required by browser); disabled = prevents double connection attempts
    <Button size="lg" onClick={connect} disabled={status === "connecting"}>
      {status === "idle" && "Connect device"}
      {status === "connecting" && "Connecting…"}
      {status === "connected" && "Connected"}
    </Button>
  );
}
