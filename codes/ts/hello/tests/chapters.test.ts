import assert from "node:assert/strict";
import test from "node:test";
import { CHAPTERS } from "../chapterRegistry.js";

for (const chapter of CHAPTERS) {
  test(`${chapter.title} runs`, async () => {
    await chapter.run();
    assert.ok(chapter.path.endsWith(".ts"));
  });
}
