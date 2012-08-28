
mapeditor: mapeditor.c data/tiles_big.png
	$(CC) -std=gnu99 -lSDL -lSDL_image -lSDL_gfx mapeditor.c -o mapeditor

data/tiles_big.png: data/tiles.png
	convert data/tiles.png -scale 400%x300% data/tiles_big.png

clean:
	-rm *.o mapeditor
