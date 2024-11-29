const fs = require("fs");
const path = require("path");
const xml2js = require("xml2js");
const Ajv = require("ajv");
const addFormats = require("ajv-formats");
const schema = require("./spinJsonSchema");

// JSON Schema

// Initialize AJV
const ajv = new Ajv();
addFormats(ajv);
const validate = ajv.compile(schema);

function convertXMLToJSON(xmlFilePath, outputJsonPath) {
	return new Promise((resolve) => {
		fs.readFile(xmlFilePath, "utf-8", (err, data) => {
			if (err) {
				console.error(`Error reading XML file ${xmlFilePath}:`, err);
				resolve(false);
				return;
			}

			// Parse XML to JS Object
			const parser = new xml2js.Parser({
				explicitArray: false,
				mergeAttrs: true,
			});
			parser.parseString(data, (err, result) => {
				if (err) {
					console.error(`Error parsing XML file ${xmlFilePath}:`, err);
					resolve(false);
					return;
				}

				// Transform into JSON matching schema
				const jsonData = transformToSchema(result);

				if (!validate(jsonData)) {
					console.error(
						`Validation failed for file ${xmlFilePath}:`,
						validate.errors
					);
					resolve(false);
					return;
				}

				console.log(`Schema validation passed for the json of ${xmlFilePath}`);

				// Write JSON file
				fs.writeFile(
					outputJsonPath,
					JSON.stringify(jsonData, null, 2),
					(err) => {
						if (err) {
							console.error(`Error writing JSON file ${outputJsonPath}:`, err);
							resolve(false);
						} else {
							console.log(`Converted ${xmlFilePath} -> ${outputJsonPath}`);
							resolve(true);
						}
					}
				);
			});
		});
	});
}

// Converter Function
function convertXMLToJSONOLD(xmlFilePath, outputJsonPath) {
	fs.readFile(xmlFilePath, "utf-8", (err, data) => {
		if (err) {
			console.error(`Error reading XML file ${xmlFilePath}:`, err);
			return;
		}

		// Parse XML to JS Object
		const parser = new xml2js.Parser({
			explicitArray: false,
			mergeAttrs: true,
		});
		parser.parseString(data, (err, result) => {
			if (err) {
				console.error(`Error parsing XML file ${xmlFilePath}:`, err);
				return;
			}

			// Transform into JSON matching schema
			const jsonData = transformToSchema(result);

			if (!validate(jsonData)) {
				console.error(
					`Validation failed for file ${xmlFilePath}:`,
					validate.errors
				);
				return;
			}

			console.log(`Validation passed for file ${xmlFilePath}`);
			// Write JSON file
			fs.writeFile(outputJsonPath, JSON.stringify(jsonData, null, 2), (err) => {
				if (err) {
					console.error(`Error writing JSON file ${outputJsonPath}:`, err);
				} else {
					console.log(`Converted ${xmlFilePath} -> ${outputJsonPath}`);
				}
			});
		});
	});
}

// Transformation Logic: Adjust according to schema
function transformToSchema(xmlObject) {
	const spinSystem = xmlObject.spin_system || {};

	const result = {
		spin: [],
		interaction: [],
	};

	if (spinSystem.spin) {
		const spins = Array.isArray(spinSystem.spin)
			? spinSystem.spin
			: [spinSystem.spin];
		spins.forEach((spin) => {
			const spinData = {
				id: parseInt(spin.id),
				isotope: spin.isotope,
				label: spin.label || undefined,
				coordinates: spin.coordinates
					? {
							x: parseFloat(spin.coordinates.x),
							y: parseFloat(spin.coordinates.y),
							z: parseFloat(spin.coordinates.z),
					  }
					: undefined,
			};
			result.spin.push(spinData);
		});
	}

	if (spinSystem.interaction) {
		const interactions = Array.isArray(spinSystem.interaction)
			? spinSystem.interaction
			: [spinSystem.interaction];
		interactions.forEach((interaction) => {
			const interactionData = {
				kind: interaction.kind,
				id: parseInt(interaction.id),
				units: interaction.units,
				spin_a: parseInt(interaction.spin_a),
				spin_b: interaction.spin_b ? parseInt(interaction.spin_b) : undefined,
				reference: interaction.reference || undefined,
				label: interaction.label || undefined,
			};

			if (interaction.scalar) {
				interactionData.scalar = {
					iso: parseFloat(interaction.scalar.iso),
				};
			}

			if (interaction.tensor) {
				interactionData.tensor = {
					xx: parseFloat(interaction.tensor.xx),
					xy: parseFloat(interaction.tensor.xy),
					xz: parseFloat(interaction.tensor.xz),
					yx: parseFloat(interaction.tensor.yx),
					yy: parseFloat(interaction.tensor.yy),
					yz: parseFloat(interaction.tensor.yz),
					zx: parseFloat(interaction.tensor.zx),
					zy: parseFloat(interaction.tensor.zy),
					zz: parseFloat(interaction.tensor.zz),
				};
			}

			result.interaction.push(interactionData);
		});
	}

	return result;
}

// Convert All XML Files in the Examples Directory
const inputDir = "./examples";
const outputDir = "./output";

// Ensure Output Directory Exists
if (!fs.existsSync(outputDir)) {
	fs.mkdirSync(outputDir, { recursive: true });
}

// Read All XML Files from Input Directory
var numberOK = 0;
var numberFailed = 0;
// Read All XML Files from Input Directory
fs.readdir(inputDir, async (err, files) => {
	if (err) {
		console.error("Error reading input directory:", err);
		console.log(`\nSummary: 0 files passed, 1 files failed.`);
		return;
	}

	const xmlFiles = files.filter((file) => {
		const ext = path.extname(file).toLowerCase();
		return ext === ".xml" || ext === ".sxml";
	});

	if (xmlFiles.length === 0) {
		console.log("No XML files found in the input directory.");
		console.log(`\nSummary: 0 files passed, 1 files failed.`);
		return;
	}

	let numberOK = 0;
	let numberFailed = 0;

	// Process each XML file and count results
	const results = await Promise.all(
		xmlFiles.map((file) => {
			const jsonFileName = file.replace(".xml", ".json");
			const outputJsonPath = path.join(outputDir, jsonFileName);
			const xmlFilePath = path.join(inputDir, file);
			return convertXMLToJSON(xmlFilePath, outputJsonPath);
		})
	);

	// Count OK and Failed
	results.forEach((result) => {
		if (result) {
			numberOK++;
		} else {
			numberFailed++;
		}
	});

	// Display Summary
	console.log(
		`\nSummary: ${numberOK} files passed, ${numberFailed} files failed.`
	);
});
