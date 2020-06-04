library(ggplot2)
library(scales)
library(doBy)
library(dplyr)
library(readr)

setwd("~/Desktop/ConsumerCbr/Congestion/FIFO/CacheHits")

#freq = c(900,450,225,112,56,40,28)

#freq=40

cs <- list() # creates a list
list_cs<- dir(pattern = "*.txt") # creates the list of all the .txt files in the directory
for (k in 1:length(list_cs))
{
  cs[[k]] = read.table(list_cs[k],header=T)
}

for (k in 1:length(list_cs))
{
  cs[[k]] =  subset(cs[[k]], Type %in% c("CacheHits"))
  cs[[k]] =  subset(cs[[k]], Node %in% c( "Rtr1","Consumer1","Consumer2"))
  cs[[k]] =  subset(cs[[k]], select = -c(Time, Type)) #Node
  cs[[k]]$ID = k
}



for (k in 1:length(list_cs))
{
 # cs[[k]]$Packets = cs[[k]]$Packets/freq[[k]]
   cs[[k]]$Packets = cs[[k]]$Packets/freq
}

csfinale = bind_rows(cs)
csfinale = csfinale[csfinale$Packets != 0, ]

Thr=summarise(group_by(csfinale, ID, Node), value = mean(Packets))
write_tsv(Thr, path = "cachehits.txt")

