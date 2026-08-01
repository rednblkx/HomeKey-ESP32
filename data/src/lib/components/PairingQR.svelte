<script lang="ts">
	import qrcode from 'qrcode-generator';

	interface Props {
		/** 8-digit HomeKit setup code, e.g. "46637726". */
		setupCode: string;
		/** 4-character Setup ID. HomeSpan's default is "HSPN" unless setQRID() is called. */
		setupId?: string;
		/** HAP accessory category. HomeKey-ESP32 registers as Category::Locks = 6. */
		category?: number;
	}

	let { setupCode, setupId = 'HSPN', category = 6 }: Props = $props();

	// HAP setup payload, per the HomeKit Accessory Protocol specification:
	//
	//   bits 43..45  version   (0)
	//   bits 39..42  reserved  (0)
	//   bits 31..38  category
	//   bits 27..30  flags     (2 = IP/WiFi transport)
	//   bits  0..26  setup code
	//
	// The 9-byte value is base36-encoded, zero-padded to 9 characters, and
	// prefixed with "X-HM://". The 4-character Setup ID is appended.
	//
	// BigInt is required: the payload exceeds 32 bits, and shifting past bit 31
	// with normal JS numbers silently wraps.
	const payloadUri = $derived.by(() => {
		const digits = (setupCode ?? '').replace(/\D/g, '');
		if (digits.length !== 8) return null;

		const code = BigInt(digits);
		const value =
			(BigInt(category) << 31n) | (2n << 27n) | code;

		let encoded = '';
		let n = value;
		const alphabet = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ';
		while (n > 0n) {
			encoded = alphabet[Number(n % 36n)] + encoded;
			n = n / 36n;
		}
		encoded = encoded.padStart(9, '0');

		return `X-HM://${encoded}${setupId}`;
	});

	const svg = $derived.by(() => {
		if (!payloadUri) return null;
		// Type 0 = auto-size. 'M' error correction matches what Apple's own
		// setup labels use.
		const qr = qrcode(0, 'M');
		qr.addData(payloadUri);
		qr.make();
		return qr.createSvgTag({ cellSize: 4, margin: 4, scalable: true });
	});

	const formattedCode = $derived.by(() => {
		const d = (setupCode ?? '').replace(/\D/g, '');
		return d.length === 8 ? `${d.slice(0, 3)}-${d.slice(3, 5)}-${d.slice(5)}` : setupCode;
	});
</script>

<div class="py-2 px-3 bg-base-100 rounded-lg">
	<p class="text-sm font-medium mb-2">HomeKit Pairing</p>

	{#if svg}
		<div class="flex flex-col sm:flex-row items-center gap-4">
			<!-- White backing: QR scanners need light quiet-zone contrast, which a
			     dark UI theme would otherwise destroy. -->
			<div class="bg-white p-2 rounded-lg shrink-0 w-40 h-40 [&>svg]:w-full [&>svg]:h-full">
				{@html svg}
			</div>
			<div class="text-center sm:text-left">
				<p class="text-xs opacity-60 mb-1">Setup code</p>
				<p class="text-2xl font-mono tracking-wider mb-2">{formattedCode}</p>
				<p class="text-xs opacity-60">
					Scan with the Home app, or enter the code manually via
					<span class="whitespace-nowrap">Add Accessory &rarr; More options</span>.
				</p>
				<p class="text-xs opacity-40 mt-2 font-mono break-all">{payloadUri}</p>
			</div>
		</div>
	{:else}
		<p class="text-xs opacity-60">
			Set an 8-digit setup code to generate a pairing QR code.
		</p>
	{/if}
</div>
