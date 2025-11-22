import adapter from '@sveltejs/adapter-static';
import { execSync } from 'node:child_process';

/** @type {import('@sveltejs/kit').Config} */
const config = {
  kit: {
    adapter: adapter({
      pages: 'build',
      assets: 'build',
      fallback: 'app.html',
      precompress: true,
      strict: true
    }),
    output: {
      bundleStrategy: 'single',
    },
    version: {
      name: execSync('git rev-parse --short=16 HEAD').toString().trim()
    }
  }
};

export default config;
