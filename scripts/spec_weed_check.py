#!/usr/bin/env python3
"""gybis-spec-weed: spec-to-code divergence analysis."""
import os, re, sys

base = "/home/pauwhi/Downloads/junk/cljonic-playground"
spec_dir = f"{base}/specs/collections"
test_dir = f"{base}/tests"
src_dir = f"{base}/src"

# 1. Extract all invariants from each .allium file
entities = {}  # {entity_name: [invariant_names]}
for fname in sorted(os.listdir(spec_dir)):
    if not fname.endswith(".allium"):
        continue
    entity_name = fname[:-7]  # strip .allium
    cap_name = entity_name.capitalize()
    invariants = []
    with open(f"{spec_dir}/{fname}") as fh:
        for line in fh:
            m = re.match(r'^( +)invariant\.([A-Za-z]+)\s*\{', line)
            if m:
                invariants.append(m.group(2))
    entities[entity_name] = (cap_name, invariants)

print(f"Spec entities found: {len(entities)}")
for name, (cap, invs) in sorted(entities.items()):
    print(f"  {cap}: {len(invs)} invariants ({name})")

# 2. Extract all TRACE_ID values from test files
trace_ids = {}  # {test_file: set of trace-id strings}
for fname in sorted(os.listdir(test_dir)):
    if not (fname.startswith("cljonic-") and fname.endswith("-spec-tests.cpp")):
        continue
    ids = set()
    with open(f"{test_dir}/{fname}") as fh:
        for line in fh:
            m = re.search(r'TRACE_ID\("([^"]+)"', line)
            if m:
                ids.add(m.group(1))
    trace_ids[fname] = ids

print(f"\nTest files: {len(trace_ids)}")
for tf, ids in sorted(trace_ids.items()):
    # Count invariant-related trace ids
    inv_trace = [t for t in ids if "invariant." in t or "entity-fields" in t]
    print(f"  {tf}: {len(inv_trace)} trace IDs")

# 3. Compare: every spec invariant should have at least one matching TRACE_ID
divergences = []
print("\n=== DIVERGENCE REPORT ===")
print("\n--- Spec → Test Coverage ---")
for entity_name in sorted(entities.keys()):
    cap_name, invariants = entities[entity_name]
    
    # Find matching test file(s)
    matching_tests = [tf for tf in trace_ids 
                      if f"invariant.{cap_name}." in ' '.join(trace_ids[tf])]
    
    untraced_invariants = []
    traced_count = 0
    
    for inv in invariants:
        pattern = f"invariant.{cap_name}.{inv}"
        found = any(pattern in tid for tid in trace_ids.get(matching_tests[0], []) if matching_tests)
        # Also check if any test file has this pattern
        if not found:
            for tf, ids in trace_ids.items():
                if any(pattern in tid for tid in ids):
                    found = True
                    break
        
        if found:
            traced_count += 1
        else:
            untraced_invariants.append(inv)
            divergences.append({
                "type": "obligation_missing_in_code",
                "entity": cap_name,
                "invariant": inv,
                "message": f"{pattern} not found in test TRACE_IDs"
            })
    
    status = "✓" if not untraced_invariants else "✗"
    print(f"  {status} {cap_name}: {traced_count}/{len(invariants)} traced")
    for u in untraced_invariants:
        print(f"      ✗ UNTRACED: invariant.{cap_name}.{u}")

# 4. Check code methods vs spec operations
print("\n--- Code Methods (public API) ---")
expected_ops = {
    "Vector": {"capacity", "size", "empty", "valid_index", "operator()", "operator[]"},
    "MapEntry": {"key", "value", "valid_index", "operator=="},
    "Map": {"capacity", "size", "empty", "contains", "operator()", "assoc", "dissoc"},
    "Set": {"capacity", "size", "count", "empty", "contains", "operator()", "can_conj", "conj", "disj"},
    "Queue": {"capacity", "size", "empty", "can_conj", "conj", "peek", "pop"},
    "String": {"capacity", "size", "empty", "valid", "operator[]", "operator()", "put", "append"},
}

for fname in sorted(os.listdir(src_dir)):
    if not fname.endswith(".hpp"):
        continue
    
    with open(f"{src_dir}/{fname}") as fh:
        content = fh.read()
    
    # Extract class/struct name
    class_match = re.search(r'(?:class|struct)\s+(\w+)', content)
    if not class_match:
        continue
    class_name = class_match.group(1)
    
    # Find public methods (inside public section, before private)
    public_section = re.search(r'public:(.*?)(?:private:|$)', content, re.DOTALL)
    if public_section:
        methods = re.findall(r'\[\[nodiscard\]\].*?(\w+)\s*\(', public_section.group(1))
        methods = sorted(set([m for m in methods if m != 'operator==']))
        
        expected = expected_ops.get(class_name, set())
        extra = [m for m in methods if m not in expected and m != 'find_index']
        missing = expected - set(methods)
        # constructor is usually not named but exists
        
        print(f"  {fname}: {methods}")
        if extra:
            print(f"    ⚠ Extra (unspecified?): {extra}")
        if missing:
            print(f"    ⚠ Missing from spec?: {missing}")

print("\n=== RESULT ===")
if divergences:
    print(f"DIVERGENCES FOUND: {len(divergences)}")
    for d in divergences[:5]:
        print(f"  - {d['message']}")
else:
    print("NO DIVERGENCES — SPEC-CODE CONVERGED ✓")
