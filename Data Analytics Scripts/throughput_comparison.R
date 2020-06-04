library(ggplot2)
library(scales)
library(doBy)
library(dplyr)
library(readr)


#setwd("~/Desktop/ConsumerCbr/Congestion/RANDOM/Through")
  
  through <- list() # creates a list
  list_throughput<- dir(pattern = "*.txt") # creates the list of all the .txt files in the directory
  for (k in 1:length(list_throughput))
  {
    through[[k]] <- read.table(list_throughput[k],header=T)
  }
  
  for (k in 1:length(list_throughput))
  {
    through[[k]]$Kilobits = through[[k]]$Kilobytes * 8
  }
  
  for (k in 1:length(list_throughput))
  {
    
    through[[k]] = subset( through[[k]], Type %in% c( "InData" ))
    through[[k]] = subset( through[[k]], Node %in% c("Rtr1"))
    through[[k]]$ID = k
    through[[k]]= summaryBy(. ~ Time + Node + Type + ID, data = through[[k]], FUN=sum)
    through[[k]]= subset( through[[k]], select = -c(FaceId.sum,Node, Type, Packets.sum, Kilobytes.sum, KilobytesRaw.sum))  
  }
  
  
  through_finale = bind_rows(through)
  
  g.all <- ggplot(data=through_finale,aes (x=Time, y=Kilobits.sum, colour=ID), size=1) +
    geom_line()+
    geom_point() +
    xlab("Time")+
    ylab("Rate [kbits]")+
    facet_wrap(~ID, scales = "free")
  
  print(g.all)
  
  Thr=summarise(group_by(through_finale, ID), value = mean(Kilobits.sum))
  sapply(through_finale, class)
write_tsv(Thr, path = "throughput.txt")










