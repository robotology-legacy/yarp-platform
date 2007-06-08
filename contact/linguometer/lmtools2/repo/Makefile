# Usage:
#  make clean
#  make prepare 
#  make process
CFG_NICE=config/lm_nice
VAL_NICE=`if [ -e "$(CFG_NICE)" ]; then cat $(CFG_NICE); else echo 0; fi`
CMD_NICE=/usr/bin/nice -n $(VAL_NICE)

all: prepare process
process: collect detect segment align aligncc plot

clean:
	@echo "Cleaning"
	@$(CMD_NICE) lm_clean --noask

prepare:	
	@echo "Preparing data for segment identification"
	@$(CMD_NICE) lm_prepare

collect:
	@echo "Collecting data"
	@$(CMD_NICE) lm_collect
	@$(CMD_NICE) lm_exportamp
	@$(CMD_NICE) lm_exportpos

detect:
	@echo "Performing peak-detection"
	@$(CMD_NICE) lm_pdetect2

segment:
	@echo "Performing segmentation"
	@$(CMD_NICE) lm_segment

align:
	@echo "Performing US<-->AG alignment"
	@$(CMD_NICE) lm_walign2
	@$(CMD_NICE) lm_splitamp
	@$(CMD_NICE) lm_splitpos

aligncc:
	@echo "Performing US<-->CC alignment"
	@$(CMD_NICE) lm_cleancc
	@$(CMD_NICE) lm_postcc
	@$(CMD_NICE) lm_segmentcc

plot:
	@$(CMD_NICE) lm_plotlog
