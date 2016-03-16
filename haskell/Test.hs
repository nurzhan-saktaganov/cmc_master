{-# LANGUAGE FlexibleInstances #-}
newtype Tup a b = Tup (a, b)

instance Show (Tup Int Int) where
  show (Tup tup) = show tup

instance Show (Tup String Int) where
  show (Tup (s, int)) = s ++ ": " ++ show int

