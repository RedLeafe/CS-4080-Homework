-- Complementary pattern for Haskell
-- Each type is a record of functions implementing its operations
-- Each constructor returns a set of behaviors, new types can be added without changing code

data Expr = Expr { pretty :: String, eval :: Int }

lit :: Int -> Expr
lit n = Expr { pretty = show n, eval = n }

add :: Expr -> Expr -> Expr
add a b = Expr { pretty = "(" ++ pretty a ++ " + " ++ pretty b ++ ")" , eval   = eval a + eval b }

mul :: Expr -> Expr -> Expr
mul a b = Expr { pretty = "(" ++ pretty a ++ " * " ++ pretty b ++ ")" , eval   = eval a * eval b }