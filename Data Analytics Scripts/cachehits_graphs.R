library(ggplot2)
library(scales)
library(doBy)
library(dplyr)
library(readr)


cs <- list() # creates a list
list_cs<- dir(pattern = "*.txt") # creates the list of all the .txt files in the directory
for (k in 1:length(list_cs))
{
  cs[[k]] = read.table(list_cs[k],header=T)
}


for (k in 1:length(list_cs))
{
  cs[[k]] =  subset(cs[[k]], Node %in% c( "Rtr1" ))
  cs[[k]] =  subset(cs[[k]], Type %in% c("CacheHits"))
  cs[[k]]$ID = k
}

csfinal <- bind_rows(cs)

g.all <- ggplot(csfinal, aes (x=Time, y=Packets, colour=Node), size=1) +
  geom_point() +
  ylab("CacheHits-Packets")  +
  facet_wrap(~ ID, scales="free")
print(g.all)