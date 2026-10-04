# One-Step-GWAS

## One-Step-GWAS Test Release (2026-10-03)

This test release includes the following updates:

### 1. Input Data Help Indicator

- Replaced the question-mark help icon with an information icon **ⓘ** for clearer input format guidance.

### 2. Data Format Support

- Added support for **Numeric** genotype format in both single-file and multi-file workflows.
- Updated the display name of the **BLINK** genotype format.
- Existing supported formats:
  - BLINK
  - HapMap
  - Numeric
  - PLINK
  - VCF
  - Simplified Variants

### 3. GWAS Analysis Support

- Added multi-file support for the **BLINK** algorithm, with results verified to be consistent with single-file analysis.
- The software currently provides two built-in GWAS methods: **GLM** and **BLINK**.
- GWAS analysis is supported for both single and multiple genotype files across multiple supported data formats.
- To improve compatibility and usability with large datasets, some algorithms and plotting functions that were not suitable for large-data workflows have been removed.

### 4. Visualization Improvements

- Added support for point size control in Manhattan plots.
- Fixed point color control in Manhattan plots.
- Fixed point color control in Q-Q plots.
- Manhattan plots support:
  - Single-color display
  - Multi-color alternating display across chromosomes

### 5. Test Packages

This release includes:

- `DEMO_DATA.zip`
- `One-Step-GWAS-Windows-portable.zip`
- `One-Step-GWAS-macOS-arm64.dmg`

### 6. Platform Notes

- **Windows:** Portable version available for direct testing.
- **macOS:** Tested on Apple Silicon (M-series, arm64).
- This is a test release intended for functionality validation.
