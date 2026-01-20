mogrify -resize 800x800\> -strip *.jpg */*.jpg
jpegoptim --max=85 *.jpg */*.jpg
mogrify -resize 800x800\> -strip *.png */*.png
optipng -o7 *.png */*.png

