t <- c(-8.338968 , -9.113330 , -9.510013 , -9.790255 , -9.978659 ,
-10.202580 , -10.401131 , -10.443087 , -10.558075 , -10.681729 ,
-10.844045 , -10.970011 , -11.034489 , -11.108202 , -11.229610 ,
-11.247724 , -11.249417 , -11.317318 , -11.393747 , -11.451436)
n <- seq(100000, 2000000, 100000)
n2 <- log(n)
m <- lm(t~n2)
s <- summary(m)
plot(t~n2, xlim=c(11,16), ylim=c(-13,-8))
abline(s)
print(m)

# Results:
#   If yhat = ln(1-eta) then yhat = -1.035*ln(n) + 3.562
#   where n is the size of the input.  This predicts the
#   efficiency based on random data as input.

points(log(6862108), -1.035*log(6862108) + 3.562, col="red")

