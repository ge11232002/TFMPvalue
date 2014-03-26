TFMpv2sc <- function(mat, pvalue, bg=c(A=0.25, C=0.25, G=0.25, T=0.25),
                     type=c("PFM", "PWM")){
  ## TODD: valiate these mat, bg
  type <- match.arg(type)
  if(length(pvalue) != 1L){
    stop("pvalue must be length of 1")
  }
  score <- .Call("pv2sc", mat, pvalue, bg, type)
  return(score)
}

