export function Hero() {
  return (
      <div className="flex flex-col items-center text-center gap-8 pt-2 pb-16 sm:pt-0 sm:pb-20">
      {/* Title */}
      <h1 className="text-5xl sm:text-8xl font-bold tracking-tight">
        Smart Al-Test
      </h1>

      {/* Subtitle */}
      <p className="text-lg sm:text-xl text-gray-300 max-w-xl">
        Know your alcohol level before it becomes a risk!
      </p>

      {/* Description (wraps into 2 lines + centered) */}
      <p className="text-base sm:text-lg text-gray-400 max-w-lg mx-auto">
        A BLE-enabled smart breathalyzer that securely stores 
        <br className="hidden sm:block" />
        readings in your personal cloud dashboard.
      </p>

      {/* Divider (optional, smaller spacing) */}
      <div className="w-full max-w-2xl h-px bg-gradient-to-r from-transparent via-white/10 to-transparent mt-10" />
    </div>
  );
}