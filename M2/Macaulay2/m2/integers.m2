--		Copyright 1993-2002 by Daniel R. Grayson

ZZ#1 = 1
ZZ#0 = 0
ZZ.char = 0
ZZ.InverseMethod = x -> 1/x
ZZ.dim = 1
ZZ.Engine = true
ZZ.baseRings = {}
ZZ.degreeLength = 0
ZZ.frac = QQ
ZZ.RawRing = rawZZ()
degree ZZ := i -> {}
lift = method()
liftable = method()
promote = method()
promote(ZZ,ZZ) := (i,ZZ) -> i
lift(ZZ,ZZ) := (i,ZZ) -> i
ZZ.random = () -> random 21 - 10

ZZ >> ZZ := ZZ => (i,j) -> i << -j

oldgcd := gcd
erase symbol gcd
gcd = method(Associative => true)
-- lcm = method(Associative => true)

gcd List := x -> gcd toSequence x
-- lcm List := x -> lcm toSequence x

gcd(ZZ,ZZ) := ZZ => (x,y) -> oldgcd(x,y)
-- lcm(ZZ,ZZ) := ZZ => (x,y) -> x*y//gcd(x,y)

-- powermod := (m,e,p) -> m^e % p;

powermod := (m,e,p) -> lift((m + 0_(ZZ/p))^e,ZZ)

smallprimes := {2,3,5,7,11,13,17,23,29,31,37,41,43,47}

isPrime1 := n -> member(n,smallprimes) or all(smallprimes,p -> n%p =!= 0)

isPrime2 := n -> (			  -- assume n > 2
     a := 2;				  -- base for pseudo-primality
     n1 := n-1;
     n2 := 1;
     while even n1 do (n1 = n1//2; n2 = 2*n2;);
     kk := (-1) % n;
     k := powermod(a,n1,n);
     while k =!= 0 and k =!= 1 and n2 > 1 do (
	  kk = k;
	  k = k^2 % n;
	  n2 = n2 // 2;
	  );
     k === 1 and kk === (-1) % n)

biggest := 2^31-1

isPrime ZZ := Boolean => n -> (
     n = abs n;
     n > 1 and
     if n <= biggest 
     then (
	  v := factor n;
	  # v === 1 and v#0#1 === 1
	  )
     else isPrime1 n and (n == 2 or isPrime2 n)
     )

random ZZ := ZZ => x -> (
     if x <= 0 then error "expected a positive number";
     randomint() % x)

d := 2^31 - 1.

random RR := RR => x -> (
     if x <= 0. then (
	  error "expected a positive number"
	  );
     x * (randomint() / d))

erase symbol randomint

ceiling = x -> - floor(-x)

isUnit ZZ := x -> x == 1 or x == -1

ZZ & ZZ := ZZ => lookup(symbol &, ZZ, ZZ)

isConstant ZZ := i -> true

raw ZZ := x -> rawFromNumber(rawZZ(), x)
