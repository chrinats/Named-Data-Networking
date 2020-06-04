library(ggplot2)
library(scales)
library(doBy)
library(dplyr)
library(readr)

#setwd("~/Desktop/ConsumerCbr/Congestion/FIFO/Delay")

delay <- list() # creates a list

list_delay<- dir(pattern = "*.txt") # creates the list of all the .txt files in the directory

  for (k in 1:length(list_delay))
{
  delay[[k]] <- read.table(list_delay[k],header=T)
}


  for (k in 1:length(list_delay))
{
  delay[[k]] =  subset(delay[[k]], Type %in% c( "FullDelay" ))
  delay[[k]] =  subset(delay[[k]], select = -c(Time,AppId, SeqNo, Type, DelayUS, RetxCount, HopCount))
  delay[[k]]$ID = k
}  



delayfinale = bind_rows(delay)
delayfinale = summaryBy(. ~ID+Node, data=delayfinale, FUN=mean)

write_tsv(delayfinale, path = "delaymean.txt")