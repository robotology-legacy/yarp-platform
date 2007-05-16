#!/bin/bash
EXP="1"
rm temp/wd_*
./word_align --sequence data/exp_000$EXP/ag_0000.wav --wd data/exp_000$EXP/wd_0000.wav --num 0000 --out temp/wd_0000.ag.wav --log temp/wd_0000.aln; echo "----";
./word_align --sequence data/exp_000$EXP/ag_0000.wav --wd data/exp_000$EXP/wd_0001.wav --num 0001 --out temp/wd_0001.ag.wav --log temp/wd_0001.aln; echo "----";
./word_align --sequence data/exp_000$EXP/ag_0000.wav --wd data/exp_000$EXP/wd_0002.wav --num 0002 --out temp/wd_0002.ag.wav --log temp/wd_0002.aln; echo "----";
./word_align --sequence data/exp_000$EXP/ag_0000.wav --wd data/exp_000$EXP/wd_0003.wav --num 0003 --out temp/wd_0003.ag.wav --log temp/wd_0003.aln; echo "----";

