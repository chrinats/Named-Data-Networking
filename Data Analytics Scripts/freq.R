library(ggplot2)
library(scales)
library(doBy)
library(dplyr)
library(readr)

#setwd("~/Desktop/Results/Zipf/No Congestion/FIFO/Time")

x = vector();
cstime<- list();
list_cstime<- dir(pattern = "*") # creates the list of all the .txt files in the directory

for (k in 1:length(list_cstime))
{
  cstime[[k]] <- read.table(list_cstime[k],header=F)
  # cstime[[k]] <- lapply(cstime[[k]], setNames, colnames)
  cstime[[k]]$ID <- k
}

csfinale <-  bind_rows(cstime)
csfinale=csfinale[!grepl("/localhost", csfinale$V1),]
csfinale=csfinale[!grepl("Simulation", csfinale$V1),]


entries = csfinale %>% group_by(ID,V2) %>% tally()
entries$V2 = gsub("s","",entries$V2)
entries$V2 = sapply(entries$V2, as.numeric)
entries$n  = sapply(entries$n, as.numeric)


finale=summarise(group_by(entries, ID), value = weighted.mean(V2,n))

write_tsv(finale, path = "entries.txt")





