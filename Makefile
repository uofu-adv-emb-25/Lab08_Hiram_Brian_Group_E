can:
	cmake --build build
	sudo picotool reboot -f -u
	sleep 3
	sudo picotool load -f build/src/can.uf2
	sudo picotool reboot -f