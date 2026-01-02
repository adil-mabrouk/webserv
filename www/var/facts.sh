#!/bin/bash
echo "Content-type: text/html"$'\r'
echo $'\r'

FACTS=(
  "Khouribga is known as the phosphate capital of Morocco."
  "Morocco is home to the world's largest solar power plant, Noor Ouarzazate."
  "The traditional Moroccan dish, tagine, is both a meal and the name of the pot it's cooked in."
  "The Hassan II Mosque in Casablanca has the world's tallest minaret."
  "Khouribga hosts an annual International Festival of Phosphate."
)

RANDOM_FACT=${FACTS[$RANDOM % ${#FACTS[@]}]}

echo "<html>"
echo "<head><title>Fun Fact About Morocco</title></head>"
echo "<body>"
echo "<h1>Did You Know?</h1>"
echo "<p>$RANDOM_FACT</p>"
echo "</body>"
echo "</html>"
