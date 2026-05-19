import js from "@eslint/js";
import { defineConfig, globalIgnores } from "eslint/config";
import tseslint from "typescript-eslint";

export default defineConfig(
    globalIgnores([
        "dist/**",
        "node_modules/**",
        "coverage/**",
    ]),
    {
        files: ["*.js", "*.mjs", "*.cjs"],
        extends: [js.configs.recommended],
        languageOptions: {
            ecmaVersion: "latest",
            sourceType: "module",
        },
    },
    {
        files: ["**/*.ts"],
        extends: [
            js.configs.recommended,
            tseslint.configs.recommendedTypeChecked,
        ],
        languageOptions: {
            parserOptions: {
                projectService: true,
                tsconfigRootDir: import.meta.dirname,
            },
        },
        rules: {
            "@typescript-eslint/consistent-type-exports": "error",
            "@typescript-eslint/consistent-type-imports": [
                "error",
                {
                    prefer: "type-imports",
                    fixStyle: "inline-type-imports",
                },
            ],
            "@typescript-eslint/no-import-type-side-effects": "error",
            "@typescript-eslint/no-unused-vars": [
                "error",
                {
                    argsIgnorePattern: "^_",
                    caughtErrorsIgnorePattern: "^_",
                    destructuredArrayIgnorePattern: "^_",
                    varsIgnorePattern: "^_",
                },
            ],
            "no-console": "off",
        },
    },
    {
        files: [
            "**/chapter[0-9][0-9]_*.ts",
            "**/exercises_*.ts",
        ],
        rules: {
            "@typescript-eslint/no-base-to-string": "off",
            "@typescript-eslint/no-duplicate-type-constituents": "off",
            "@typescript-eslint/no-explicit-any": "off",
            "@typescript-eslint/no-floating-promises": "off",
            "@typescript-eslint/no-for-in-array": "off",
            "@typescript-eslint/no-unnecessary-type-assertion": "off",
            "@typescript-eslint/no-unsafe-argument": "off",
            "@typescript-eslint/no-unsafe-assignment": "off",
            "@typescript-eslint/no-unsafe-function-type": "off",
            "@typescript-eslint/no-unsafe-member-access": "off",
            "@typescript-eslint/no-unused-vars": "off",
            "@typescript-eslint/require-await": "off",
            "@typescript-eslint/unbound-method": "off",
            "no-constant-condition": "off",
            "no-useless-assignment": "off",
            "no-useless-escape": "off",
            "no-var": "off",
        },
    },
    {
        files: ["tests/**/*.ts"],
        rules: {
            "@typescript-eslint/no-floating-promises": "off",
        },
    },
);
