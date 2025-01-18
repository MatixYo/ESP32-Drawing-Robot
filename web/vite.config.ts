import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react-swc';

import { viteSingleFile } from 'vite-plugin-singlefile';
import { compression } from 'vite-plugin-compression2';

// https://vite.dev/config/
export default defineConfig({
  plugins: [
    react(),

    // Bundle content into single file...
    viteSingleFile(),

    // ...and gzip it
    compression({ deleteOriginalAssets: true, skipIfLargerOrEqual: false }),
  ],
  build: {
    // Embed bigger assets directly into the output HTML
    assetsInlineLimit: 4096000,
  },
});
