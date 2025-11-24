"""
Generate a list of the most common five-letter words.
Downloads word frequency data and filters for five-letter words.
"""

import urllib.request
import re
from collections import Counter

def download_word_frequencies():
    """
    Download word frequency list from COCA (Corpus of Contemporary American English)
    via the word frequency data available on GitHub.
    """
    # Using the Google Books Ngram frequency list (top 10000 words)
    # This is a commonly used open-source word frequency list
    url = "https://raw.githubusercontent.com/first20hours/google-10000-english/master/google-10000-english-usa-no-swears.txt"
    
    print("Downloading word frequency data...")
    try:
        with urllib.request.urlopen(url) as response:
            content = response.read().decode('utf-8')
            words = content.strip().split('\n')
            print(f"Downloaded {len(words)} words")
            return words
    except Exception as e:
        print(f"Error downloading from primary source: {e}")
        print("Trying alternate source...")
        
        # Fallback to another source
        url = "https://raw.githubusercontent.com/first20hours/google-10000-english/master/google-10000-english-no-swears.txt"
        with urllib.request.urlopen(url) as response:
            content = response.read().decode('utf-8')
            words = content.strip().split('\n')
            print(f"Downloaded {len(words)} words from alternate source")
            return words

def filter_five_letter_words(words, max_words=2500):
    """
    Filter for valid five-letter words (alphabetic only, no proper nouns).
    """
    five_letter_words = []
    
    for word in words:
        word = word.strip().lower()
        # Check if word is exactly 5 letters and contains only alphabetic characters
        if len(word) == 5 and word.isalpha():
            five_letter_words.append(word)
            
            # Stop once we have enough words
            if len(five_letter_words) >= max_words:
                break
    
    return five_letter_words

def main():
    print("=" * 60)
    print("Five-Letter Word Dictionary Generator")
    print("=" * 60)
    
    # Download word frequency data
    all_words = download_word_frequencies()
    
    # Filter for five-letter words
    print("\nFiltering for five-letter words...")
    five_letter_words = filter_five_letter_words(all_words, max_words=2500)
    
    print(f"Found {len(five_letter_words)} common five-letter words")
    
    # Save to file
    output_file = "lists/ALL.TXT"
    with open(output_file, 'w') as f:
        for word in five_letter_words:
            f.write(word.upper() + '\n')
    
    print(f"\n✓ Saved {len(five_letter_words)} words to {output_file}")
    print(f"\nFirst 10 words: {', '.join(five_letter_words[:10])}")
    print(f"Last 10 words: {', '.join(five_letter_words[-10:])}")
    print("\nDone!")

if __name__ == "__main__":
    main()
