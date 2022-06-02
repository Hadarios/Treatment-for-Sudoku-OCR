all: image

image:
	@echo "starting compilation"
	@make -C Image
	@mv ./Image/image .
	@make -C Image clean
	@clear
	@echo "done!"

clean:
	@echo cleaning
	@${RM} image *.bmp
	@echo done
