mogrify -resize 800x800\> -strip walkie_talkie/*.jpg
jpegoptim --max=85 walkie_talkie/*.jpg
mogrify -resize 800x800\> -strip walkie_talkie/*.png
optipng -o7 walkie_talkie/*.png

