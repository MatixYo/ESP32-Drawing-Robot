import { defineConfig } from 'vite';
import preact from '@preact/preset-vite';

import { viteSingleFile } from 'vite-plugin-singlefile';
import { compression } from 'vite-plugin-compression2';

// https://vite.dev/config/
export default defineConfig({
  // Make websites other than the root path throw a 404
  appType: 'mpa',
  plugins: [
    preact(),

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
