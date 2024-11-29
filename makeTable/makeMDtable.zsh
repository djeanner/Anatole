#!/bin/zsh

# Check if the file is passed as an argument
if [[ $# -lt 1 ]]; then
  echo "Usage: $0 <csv_file>"
  exit 1
fi

# Input CSV file
csv_file="$1"

# Validate if the file exists
if [[ ! -f "$csv_file" ]]; then
  echo "Error: File '$csv_file' not found."
  exit 1
fi

# Function to align numbers and wrap in backticks
align_and_format() {
  local cell="$1"
  if [[ "$cell" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
    # Right-align numbers
    printf "\`%10s\`" "$cell"
  else
    # Left-align text
    printf "\`%-10s\`" "$cell"
  fi
}

# Function to process a CSV line into a Markdown table row
process_line() {
  local line="$1"
  IFS=',' read -rA columns <<< "$line"
  local row="|"
  for col in "${columns[@]}"; do
    row+=" $(align_and_format "$col") |"
  done
  echo "$row"
}

# Process the CSV file
{
  # Read the first line as the header
  read header_line
  process_line "$header_line"

  # Generate the Markdown table separator
  IFS=',' read -rA header_columns <<< "$header_line"
  echo "| $(printf ' %-10s |' $(for _ in "${header_columns[@]}"; do echo '---'; done))"

  # Process the remaining lines
  while read line; do
    process_line "$line"
  done
} < "$csv_file"

exit 0
