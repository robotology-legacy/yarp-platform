CFG_NICE=config/lm_nice
VAL_NICE=`if [ -e "$(CFG_NICE)" ]; then cat $(CFG_NICE); else echo 0; fi`
CMD_NICE=/usr/bin/nice -n $(VAL_NICE)

all: build process pack repack
clean: cleantemp cleanseq
build: cleantemp prepare collect 
process: cleanseq detect segment align aligncc features plot

cleanseq:
	@echo "Cleaning seq_*/ folders"
	@$(CMD_NICE) lm_clean --noask
	
cleantemp:
	@echo "Cleaning temp/ folder"
	@$(CMD_NICE) lm_cleantemp --noask

prepare:	
	@echo "Preparing data for segment identification"
	@$(CMD_NICE) lm_prepare
	@$(CMD_NICE) lm_mergepos pos

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

features:
	@echo "Running feature extraction code (CC, US)"
	@$(CMD_NICE) lm_features

plot:
	@echo "Saving log files"
	@$(CMD_NICE) lm_plotlog

pack:
	@echo "Packing dataset for distribuition (dist_*/*, MAT files)"
	@$(CMD_NICE) lm_package

repack:
	@echo "Packing dataset for distribuition (aln_*/*)"
	@$(CMD_NICE) lm_cleanaln
	@$(CMD_NICE) lm_repack
