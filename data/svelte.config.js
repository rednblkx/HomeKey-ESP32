import adapter from '@sveltejs/adapter-static';

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
    }
  }
};

export default config;
