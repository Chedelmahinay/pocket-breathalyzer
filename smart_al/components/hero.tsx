


export function Hero() {
  const steps = [
    {n: "1", text: "Sign in or Sign up in the upper right corner"},
    {n: "2", text: "Pair your device", title: "Pair the web app to the breathalyzer"},
    {n: "3", text: "Take the test", title: "Take a breath test using the device"},
    {n: "4", text: "View results", title: "See alcohol level data in the web or device"},
  ];
  
  
  return (
      <section className="flex flex-col items-center text-center gap-8 pt-2 pb-16 sm:pt-0 sm:pb-20">
      {/* Title */}
      <h1 className="text-5xl sm:text-8xl font-bold tracking-wide">
        Smart Al-Test
      </h1>
      {/* Subtitle */}
      <p className="text-lg sm:text-xl text-gray-300 max-w-xl">
        A student-led IoT breathalyzer project at Metropolia University
      </p>
      {/* 1st Divider */}
      <div className="w-full max-w-2xl h-px bg-gradient-to-r from-transparent via-white/10 to-transparent mt-10" />
      {/*Step Ttitle*/}
      <div className="w-full max-w-5xl mt-20 px-4 sm:px-0 text-center">
        <h2 className="text-2xl sm:text-5xl font-semibold tracking-tight">
          Easy user guide
        </h2>
        <p className="mt-1 text-sm sm:text-base text-white/60 max-w-5xl">
          From account creation to viewing results in a few simple steps
        </p>
      </div>
      {/* Steps */}
      <div className="w-full max-w-5xl mt-0 grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-6 px-2 sm:px-0">
        {steps.map((s) => (
          <div
            key={s.n}
            className="rounded-2xl bg-white/5 border border-white/10  shadow-[0_10px_30px_rgba(0,0,0,0.35)] backdrop-blur-6 min-h-[160px] flex flex-col">
            <div className="inline-flex items-center justify-center w-9 h-9 rounded-full
                bg-white/10 border border-white/15 text-white font-semibold">
              {s.n.replace(".", "")}
            </div>
            <div className="mt-4 text-base sm:text-lg font-semibold leading-snug">
              
              {s.text}
            </div>
          </div>
        ))}
      </div>
      {/* 2nd Divider */}
            <div className="w-full max-w-2xl h-px bg-gradient-to-r from-transparent via-white/10 to-transparent mt-10" />

      {/* Project Description */}
      <div className="w-full max-w-5xl mt-10 px-4 sm:px-0 text-left">
        <h2 className="text-2xl sm:text-3xl font-semibold tracking-tight">
          Project Description
        </h2>
        <p className="mt-3 text-sm sm:text-base text-white/70 leading-relaxed max-w-3xl">
          Smart Al-Test is an IoT breathalyzer system that connects a BLE-enabled device
          to a web application. Users can securely sign in, pair the device, perform a
          breath test, and view stored results under their own account.
        </p>
  
        <h2 className="text-2xl sm:text-3xl font-semibold tracking-tight mt-8">
          Key Features
        </h2>
        <ul className="mt-3 list-disc list-inside text-sm sm:text-base text-white/70 leading-relaxed max-w-3xl">
          <li>User Authentication: Secure sign-up and sign-in functionality.</li>
          <li>Device Pairing: Easy pairing of the breathalyzer device via BLE.</li>
          <li>Breath Test Functionality: reliable alcohol level measurement.</li>
          <li>Data Storage: Results are stored in a database linked to user accounts.</li>
          <li>Responsive Web App: Data is accessible on computer and mobile devices for convenience.</li>
        </ul>
      </div>
      {/* 3rd Divider */}
          <div className="w-full max-w-5xl h-px bg-gradient-to-r from-transparent via-white/10 to-transparent mt-12" />

    </section>
  );
}