-- Complementary pattern for Haskell
-- Each type is a record of functions implementing its operations
-- Each constructor returns a set of behaviors, new types can be added without changing code
-- add and multiply are shown as examples of adding new types

data Expr = Expr { string :: String, int :: Int }

lit :: Int -> Expr
lit n = Expr { string = show n, int = n }

add :: Expr -> Expr -> Expr
add a b = Expr { string = "(" ++ string a ++ " + " ++ string b ++ ")" , int    = int a + int b }

mul :: Expr -> Expr -> Expr
mul a b = Expr { string = "(" ++ string a ++ " * " ++ string b ++ ")" , int    = int a * int b }