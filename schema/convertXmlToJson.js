const fs = require("fs");
const path = require("path");
const xml2js = require("xml2js");
const Ajv = require("ajv");
const addFormats = require("ajv-formats");
const schema = require("./spinJsonSchema");
const libxmljs = require("libxmljs2");

// JSON Schema

// Initialize AJV
const ajv = new Ajv();
addFormats(ajv);
const validate = ajv.compile(schema);

function validateXMLWithSchema(xmlFilePath, schemaPath) {
    const xmlContent = fs.readFileSync(xmlFilePath, "utf-8");
    const xsdContent = fs.readFileSync(schemaPath, "utf-8");

    const xmlDoc = libxmljs.parseXml(xmlContent);
    const xsdDoc = libxmljs.parseXml(xsdContent);

    const isValid = xmlDoc.validate(xsdDoc);

    if (!isValid) {
        console.log(`ERROR Validation failed for XML file ${xmlFilePath}:`);
        console.log(`==========================================================`);
        console.log(xmlDoc.validationErrors);
        console.log(`==========================================================`);

        return false;
    }

    return true;
}

function convertXMLToJSON(xmlFilePath, outputJsonPath) {
    const schemaPath = "./spinxml_schema.xsd"; // Path to your XML schema

    // Validate the XML file against the schema
    if (!validateXMLWithSchema(xmlFilePath, schemaPath)) {
        console.log(`Skipping file ${xmlFilePath} due to schema validation errors.`);
        return false; // Indicate failure
    }
            console.log(`XML schema validation passed for XML version of ${xmlFilePath}`);

    fs.readFile(xmlFilePath, "utf-8", (err, data) => {
        if (err) {
            console.log(`Error reading XML file ${xmlFilePath}:`, err);
            return false;
        }

        // Parse XML to JS Object
        const parser = new xml2js.Parser({
            explicitArray: false,
            mergeAttrs: true,
        });
        parser.parseString(data, (err, result) => {
            if (err) {
                console.log(`Error parsing XML file ${xmlFilePath}:`, err);
                return false;
            }

            // Transform into JSON matching schema
            const jsonData = transformToSchema(result);

            if (!validate(jsonData)) {
                console.log(
                    `Validation failed for file ${xmlFilePath}:`,
                    validate.errors
                );
                return false;
            }

            console.log(`Validation passed for json version of ${xmlFilePath}`);
            // Write JSON file
            fs.writeFile(outputJsonPath, JSON.stringify(jsonData, null, 2), (err) => {
                if (err) {
                    console.log(`Error writing JSON file ${outputJsonPath}:`, err);
                } else {
                    console.log(`Converted ${xmlFilePath} -> ${outputJsonPath}`);
                }
            });
        });
    });

    return true;
}


// Converter Function
function convertXMLToJSONOLD(xmlFilePath, outputJsonPath) {
	fs.readFile(xmlFilePath, "utf-8", (err, data) => {
		if (err) {
			console.log(`Error reading XML file ${xmlFilePath}:`, err);
			return;
		}

		// Parse XML to JS Object
		const parser = new xml2js.Parser({
			explicitArray: false,
			mergeAttrs: true,
		});
		parser.parseString(data, (err, result) => {
			if (err) {
				console.log(`Error parsing XML file ${xmlFilePath}:`, err);
				return;
			}

			// Transform into JSON matching schema
			const jsonData = transformToSchema(result);

			if (!validate(jsonData)) {
				console.log(
					`Validation failed for file ${xmlFilePath}:`,
					validate.errors
				);
				return;
			}

			console.log(`Validation passed for file ${xmlFilePath}`);
			// Write JSON file
			fs.writeFile(outputJsonPath, JSON.stringify(jsonData, null, 2), (err) => {
				if (err) {
					console.log(`Error writing JSON file ${outputJsonPath}:`, err);
				} else {
					console.log(`Converted ${xmlFilePath} -> ${outputJsonPath}`);
				}
			});
		});
	});
}

function transformToSchema(xmlObject) {
    const spinSystem = xmlObject.spin_system || {};

    const result = {
        spin: [],
        interaction: []
    };

    // Handle <spin> elements
    if (spinSystem.spin) {
        const spins = Array.isArray(spinSystem.spin) ? spinSystem.spin : [spinSystem.spin];
        spins.forEach((spin) => {
            const spinData = {
                id: parseInt(spin.id),
                isotope: spin.isotope,
                label: spin.label || undefined,
                coordinates: spin.coordinates
                    ? {
                          x: parseFloat(spin.coordinates.x),
                          y: parseFloat(spin.coordinates.y),
                          z: parseFloat(spin.coordinates.z)
                      }
                    : undefined
            };
            result.spin.push(spinData);
        });
    }

    // Handle <interaction> elements
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
                spin_b: interaction.spin_b ? parseInt(interaction.spin_b) : undefined
            };

            // Capture tensor information
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
                    zz: parseFloat(interaction.tensor.zz)
                };
            }

            // Capture scalar values
            if (interaction.scalar) {
                interactionData.scalar = {
                    iso: parseFloat(interaction.scalar.iso)
                };
            }

            // Capture eigenvalues
            if (interaction.eigenvalues) {
                interactionData.eigenvalues = {
                    xx: parseFloat(interaction.eigenvalues.xx),
                    yy: parseFloat(interaction.eigenvalues.yy),
                    zz: parseFloat(interaction.eigenvalues.zz)
                };
            }

            // Capture span/skew information
            if (interaction.span_skew) {
                interactionData.span_skew = {
                    iso: parseFloat(interaction.span_skew.iso),
                    span: parseFloat(interaction.span_skew.span),
                    skew: parseFloat(interaction.span_skew.skew)
                };
            }

            // Capture axiality/rhombicity information
            if (interaction.axiality_rhombicity) {
                interactionData.axiality_rhombicity = {
                    iso: parseFloat(interaction.axiality_rhombicity.iso),
                    ax: parseFloat(interaction.axiality_rhombicity.ax),
                    rh: parseFloat(interaction.axiality_rhombicity.rh)
                };
            }

            // Capture aniso/asymm information
            if (interaction.aniso_asymm) {
                interactionData.aniso_asymm = {
                    iso: parseFloat(interaction.aniso_asymm.iso),
                    aniso: parseFloat(interaction.aniso_asymm.aniso),
                    asymm: parseFloat(interaction.aniso_asymm.asymm)
                };
            }

            // Capture orientation
            if (interaction.orientation) {
                interactionData.orientation = {};

                if (interaction.orientation.euler_angles) {
                    interactionData.orientation.euler_angles = {
                        alpha: parseFloat(interaction.orientation.euler_angles.alpha),
                        beta: parseFloat(interaction.orientation.euler_angles.beta),
                        gamma: parseFloat(interaction.orientation.euler_angles.gamma)
                    };
                }

                if (interaction.orientation.angle_axis) {
                    interactionData.orientation.angle_axis = {
                        angle: {
                            phi: parseFloat(interaction.orientation.angle_axis.angle.phi)
                        },
                        axis: {
                            x: parseFloat(interaction.orientation.angle_axis.axis.x),
                            y: parseFloat(interaction.orientation.angle_axis.axis.y),
                            z: parseFloat(interaction.orientation.angle_axis.axis.z)
                        }
                    };
                }

                if (interaction.orientation.quaternion) {
                    interactionData.orientation.quaternion = {
                        re: parseFloat(interaction.orientation.quaternion.re),
                        i: parseFloat(interaction.orientation.quaternion.i),
                        j: parseFloat(interaction.orientation.quaternion.j),
                        k: parseFloat(interaction.orientation.quaternion.k)
                    };
                }

                if (interaction.orientation.dcm) {
                    interactionData.orientation.dcm = {
                        xx: parseFloat(interaction.orientation.dcm.xx),
                        xy: parseFloat(interaction.orientation.dcm.xy),
                        xz: parseFloat(interaction.orientation.dcm.xz),
                        yx: parseFloat(interaction.orientation.dcm.yx),
                        yy: parseFloat(interaction.orientation.dcm.yy),
                        yz: parseFloat(interaction.orientation.dcm.yz),
                        zx: parseFloat(interaction.orientation.dcm.zx),
                        zy: parseFloat(interaction.orientation.dcm.zy),
                        zz: parseFloat(interaction.orientation.dcm.zz)
                    };
                }
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
var numberOK = 0
var numberFailed = 0
// Read All XML Files from Input Directory
fs.readdir(inputDir, async (err, files) => {
	if (err) {
		console.log("Error reading input directory:", err);
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
      const jsonFileName = file.replace(/\.xml|\.sxml/i, ".json");
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
	console.log(`\nSummary: ${numberOK} files passed, ${numberFailed} files failed.`);
});

