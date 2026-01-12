"use client";

import { redirect } from "next/navigation";
import { createClient } from "@/lib/supabase/server";
import { InfoIcon } from "lucide-react";
import { FetchDataSteps } from "@/components/tutorial/fetch-data-steps";
import { Suspense, useState } from "react";
import { BlockButton } from "@/components/block-button";
import { BleConnectButton } from "@/components/ble-connect-button";

// async function UserDetails() {
//   const supabase = await createClient();
//   const { data, error } = await supabase.auth.getClaims();

//   if (error || !data?.claims) {
//     redirect("/auth/login");
//   }

//   return JSON.stringify(data.claims, null, 2);
// }

export default function ProtectedPage() {
  const [data, setData] = useState<number | null>(null);
  return (
    <div className="flex-1 w-full flex flex-col gap-12">
      <div className="text-center">
        <h2 className="text-4xl font-bold mb-4">
          Connect Your Device
        </h2>
      </div>
      <div className="flex items-center justify-center gap-2">
        {/* <BlockButton /> */}
        <BleConnectButton onData={setData}/>
      </div>
      <h2 className="text-2xl text-center font-bold mb-4">
        BAC: {data}%
      </h2>
    </div>
  );
}
