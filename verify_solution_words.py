#!/usr/bin/env python3
"""
Script to verify that all words in SOLUTION.TXT exist in ALL.TXT
and add any missing words to ALL.TXT.
"""

# Read SOLUTION.TXT
with open('lists/SOLUTION.TXT', 'r') as f:
    solution_words = set(word.strip().upper() for word in f if word.strip())

# Read ALL.TXT
with open('lists/ALL.TXT', 'r') as f:
    all_words = set(word.strip().upper() for word in f if word.strip())

# Find missing words
missing_words = solution_words - all_words

if missing_words:
    print(f"Found {len(missing_words)} words in SOLUTION.TXT that are missing from ALL.TXT:")
    for word in sorted(missing_words):
        print(f"  {word}")
    
    # Add missing words to ALL.TXT
    all_words_combined = sorted(all_words | missing_words)
    
    with open('lists/ALL.TXT', 'w') as f:
        for word in all_words_combined:
            f.write(word + '\n')
    
    print(f"\nAdded {len(missing_words)} missing words to ALL.TXT")
    print("ALL.TXT has been updated and re-sorted.")
else:
    print("All words in SOLUTION.TXT already exist in ALL.TXT ✓")
