import type { NextConfig } from "next";

const nextConfig: NextConfig = {
  cacheComponents: true,
  eslint: {ignoreDuringBuilds: true},
};
export default nextConfig;
