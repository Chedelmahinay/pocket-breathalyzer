"use client";
import { useCallback, useState } from "react";
import { BleConnectButton } from "@/components/ble-connect-button";
import { createClient } from "@/lib/supabase/client";

export default function ProtectedPage() {
  const [data, setData] = useState<number | null>(null);
  const [saveState, setSaveState] = useState<"idle" | "saving" | "saved" | "error">("idle");
  const [saveError, setSaveError] = useState<string | null>(null);
  const [supabase] = useState(() => createClient());

  const saveMeasurementToDatabase = useCallback(
    async (value: number) => {
      if (!Number.isFinite(value)) return; 

      setSaveState("saving");
      setSaveError(null);

      // user_id is filled automaticly by db. client sends only the bac
      const { error: insertError } = await supabase
        .from("measurements")
        .insert([{ bac: value }]);

      if (insertError) {
        setSaveState("error");
        setSaveError(`Failed to save BAC: ${insertError.message}`);
        return;
      }

      setSaveState("saved");
    },
    [supabase],
  );

  const handleData = useCallback(
    (value: number) => {
      // Update UI immediately
      setData(value);
      void saveMeasurementToDatabase(value);
    },
    [saveMeasurementToDatabase],
  );

  return (
    <div className="flex-1 w-full flex flex-col gap-12">
      <div className="text-center">
        <h2 className="text-4xl font-bold mb-4">
          Connect Your Device
        </h2>
      </div>
      <div className="flex items-center justify-center gap-2">
        <BleConnectButton onData={handleData} />
      </div>
      <h2 className="text-2xl text-center font-bold mb-4">
        BAC: {data}%
      </h2>
      <p className="text-center text-sm text-slate-500 dark:text-slate-400">
        {saveState === "saving" && "Saving latest BAC..."}
        {saveState === "saved" && "Latest BAC saved."}
        {saveState === "error" && (saveError ?? "Could not save BAC value.")}
      </p>
    </div>
  );
}
